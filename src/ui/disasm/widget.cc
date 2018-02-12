/*
 * Copyright 2018 CodiLime
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include "ui/disasm/widget.h"

namespace veles {
namespace ui {
namespace disasm {

Widget::Widget() {
  setupMocks();
  getEntrypoint();

  setWidgetResizable(true);

  rows_ = new Rows;
  arrows_ = new Arrows;

  auto split_view = new QWidget;
  auto small_lay = new QHBoxLayout;

  small_lay->setSpacing(0);
  small_lay->setMargin(0);

  small_lay->addWidget(arrows_, 0, Qt::AlignTop);
  small_lay->addWidget(rows_, 0, Qt::AlignTop);

  split_view->setLayout(small_lay);

  setWidget(split_view);
}

void Widget::setupMocks() {
  mocks::ChunkTreeFactory ctf;

  std::unique_ptr<mocks::ChunkNode> root =
      ctf.generateTree(mocks::ChunkType::FILE);
  ctf.setAddresses(root.get(), 0, 0x1000);

  std::shared_ptr<mocks::ChunkNode> sroot{root.release()};

  std::unique_ptr<mocks::MockBlob> mb =
      std::make_unique<mocks::MockBlob>(sroot);
  blob_ = std::unique_ptr<Blob>(std::move(mb));
}

void Widget::getEntrypoint() {
  entrypoint_ = blob_->getEntrypoint();

  entrypoint_watcher_.setFuture(entrypoint_);

  connect(&entrypoint_watcher_, &QFutureWatcher<Bookmark>::finished, this,
          &Widget::getWindow);
}

void Widget::getWindow() {
  Bookmark entrypoint = entrypoint_.result();
  window_ = blob_->createWindow(entrypoint, 2, 10);

  auto entries = window_->entries();
  rows_->generate(entries);
}

}  // namespace disasm
}  // namespace ui
}  // namespace veles