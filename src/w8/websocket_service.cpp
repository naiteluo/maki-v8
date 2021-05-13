//
// Created by Jiawei Tan on 2021/5/11.
//
#include <websocketpp/endpoint.hpp>
#include "websocket_service.h"

namespace w8 {

//    WebSocketService::WebSocketService(inspector::InspectorClient *client, short port) : port_(port) {
//        inspector_client_ = client;
//        debug_server_.get_alog().clear_channels(websocketpp::log::alevel::all);
//        debug_server_.get_elog().clear_channels(websocketpp::log::alevel::all);
//
//        debug_server_.set_validate_handler(
//                bind(&WebSocketService::ws_validation_handler, this, websocketpp::lib::placeholders::_1));
//        debug_server_.set_open_handler(bind(&WebSocketService::on_open, this, websocketpp::lib::placeholders::_1));
//        debug_server_.set_close_handler(bind(&WebSocketService::on_close, this, websocketpp::lib::placeholders::_1));
//        debug_server_.set_message_handler(bind(&WebSocketService::on_message, this, websocketpp::lib::placeholders::_1,
//                                               websocketpp::lib::placeholders::_2));
//        debug_server_.set_http_handler(
//                websocketpp::lib::bind(&WebSocketService::on_http, this, websocketpp::lib::placeholders::_1));
//        debug_server_.init_asio();
//        debug_server_.set_reuse_addr(true);
//
//        debug_server_.listen(this->port_);
//        debug_server_.start_accept();
//    }
//
//    WebSocketService::~WebSocketService() {}
//
//    bool WebSocketService::ws_validation_handler(websocketpp::connection_hdl hdl) {
//        return connections_.size();
//    }
//
//    void WebSocketService::on_open(websocketpp::connection_hdl hdl) {
//        assert(connections_.size() == 0);
//        connections_.emplace(hdl, inspector::InspectorChannel(*this, hdl));
//        inspector_client_->ConnectWithChannel(&connections_.find(hdl)->second);
//        message_received_time = std::chrono::high_resolution_clock::now();
//    }
//
//    void WebSocketService::on_close(websocketpp::connection_hdl hdl) {
//        assert(connections_.size() == 1);
//        connections_.erase(hdl);
//        assert(connections_.size() == 0);
//
//        inspector_client_->ResetSession();
//        inspector_client_->paused = false;
//        message_received_time = std::chrono::high_resolution_clock::now();
//    }
//
//    void WebSocketService::on_message(websocketpp::connection_hdl hdl, DebugServer::message_ptr msg) {
//        std::string message_payload = msg->get_payload();
//
//        v8_inspector::StringView message_view((uint8_t const *) message_payload.c_str(), message_payload.length());
//
//        // todo： create macros to do such thing
//        v8::Isolate *isolate = inspector_client_->getIsolate();
//        v8::Locker locker(isolate);
//        v8::Isolate::Scope isolate_scope(isolate);
//        v8::HandleScope handle_scope(isolate);
//        v8::Local<v8::Context> context = inspector_client_->GetGlobalContext().Get(isolate);
//        v8::Context::Scope context_scope(context);
//
//        inspector_client_->GetSession().dispatchProtocolMessage(message_view);
//        message_received_time = std::chrono::high_resolution_clock::now();
//    }
//
//    void WebSocketService::on_http(websocketpp::connection_hdl hdl) {
//        DebugServer::connection_ptr conn = debug_server_.get_con_from_hdl(hdl);
//        std::stringstream output;
//        output << "<!doctype html><html><body>You requested "
//               << conn->get_resource()
//               << "</body></html>";
//        conn->set_status(websocketpp::http::status_code::ok);
//        conn->set_body(output.str());
//    }


}
