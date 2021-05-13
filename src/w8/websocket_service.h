//
// Created by Jiawei Tan on 2021/5/11.
//

#ifndef MAKI_WEBSOCKET_SERVICE_H
#define MAKI_WEBSOCKET_SERVICE_H

#include "websocketpp/config/asio_no_tls.hpp"
#include "websocketpp/server.hpp"
#include <chrono>

namespace w8 {

//    typedef websocketpp::server<websocketpp::config::asio> DebugServer;
//    typedef std::map<websocketpp::connection_hdl, w8::inspector::InspectorChannel, std::owner_less<websocketpp::connection_hdl>> WebSocketConnections;
////    typedef std::map<websocketpp::connection_hdl, inspector::InspectorChannel> WebSocketConnections;
//
//    class WebSocketService {
//    public:
//
//        WebSocketService(inspector::InspectorClient *client, short port);
//
//        ~WebSocketService();
//
//
//
//        void send(websocketpp::connection_hdl &connection, const v8_inspector::StringView &message);
//
//        void wait_for_connection(std::chrono::duration<float> sleep_between_polls = std::chrono::duration<float>(0.1f));
//
//        void poll();
//
//        void poll_one();
//
//        void poll_until_idle(float idle_time = 1.0f);
//
//        void run_one();
//
//        void send_message(websocketpp::connection_hdl connection, void * data, size_t length);
//
//        float seconds_since_message_received();
//
//        float seconds_since_message_sent();
//
//        float seconds_since_message();
//
//
//    private:
//
//        bool ws_validation_handler(websocketpp::connection_hdl hdl);
//
//        // wspp callback
//
//        void on_open(websocketpp::connection_hdl hdl);
//
//        void on_close(websocketpp::connection_hdl hdl);
//
//        void on_message(websocketpp::connection_hdl hdl, DebugServer::message_ptr msg);
//
//        void on_http(websocketpp::connection_hdl hdl);
//
//        std::chrono::time_point<std::chrono::high_resolution_clock> message_received_time = std::chrono::high_resolution_clock::now();
//        std::chrono::time_point<std::chrono::high_resolution_clock> message_sent_time = std::chrono::high_resolution_clock::now();
//
//        unsigned short port_ = 0;
//        inspector::InspectorClient *inspector_client_;
//        DebugServer debug_server_;
//        WebSocketConnections connections_;
//
//    };
}

#endif //MAKI_WEBSOCKET_SERVICE_H
