// 
// application_class.cpp : Implement Application class
// 
// Copyright (C) 2017  Richard Chien <richardchienthebest@gmail.com>
// 
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
// 

#include "./application_class.h"

#include "app.h"

#include "conf/loader.h"
#include "api/server_class.h"

using namespace std;

void Application::initialize(const int32_t auth_code) {
    init_sdk(auth_code);
    initialized_ = true;
}

void Application::enable() {
    static const auto TAG = u8"启用";

    if (!initialized_ || enabled_) {
        return;
    }

    Log::d(TAG, CQAPP_FULLNAME);
    Log::d(TAG, u8"开始初始化");

    if (const auto c = load_configuration(sdk->directories().app() + "config.cfg")) {
        config = c.value();
    }

    ApiServer::instance().start(config.host, config.port);

    if (!pool) {
        Log::d(TAG, u8"工作线程池创建成功");
        pool = make_shared<ctpl::thread_pool>(config.thread_pool_size);
    }

    enabled_ = true;
    Log::i(TAG, u8"HTTP API 插件已启用");
}

void Application::disable() {
    static const auto TAG = u8"停用";

    if (!enabled_) {
        return;
    }

    ApiServer::instance().stop();

    if (pool) {
        pool->stop();
        pool = nullptr;
        Log::d(TAG, u8"工作线程池关闭成功");
    }

    enabled_ = false;
    Log::i(TAG, u8"HTTP API 插件已停用");
}
