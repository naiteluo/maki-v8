//
// Created by Jiawei Tan on 2021/5/7.
//

#ifndef MAKI_W8_INSPECTOR_H
#define MAKI_W8_INSPECTOR_H

#include "libplatform/libplatform.h"
#include "v8.h"
#include "v8-inspector.h"
#include <string>

namespace w8 {
    /**
     * inspector capability
     *
     * docs: https://v8.dev/docs/inspector
     *
     * todo: channel ()
     * todo: debugger support
     */
    namespace inspector {

        enum {
            kModuleEmbedderDataIndex, kInspectorClientIndex
        };

        inline std::string StringViewToUtf8Value(v8::Isolate *isolate, v8_inspector::StringView string_view) {
            int length = static_cast<int>(string_view.length());
            v8::Local<v8::String> string_view_str = string_view.is8Bit() ?
                                                    v8::String::NewFromOneByte(isolate,
                                                                               reinterpret_cast<const uint8_t *>(string_view.characters8()),
                                                                               v8::NewStringType::kNormal,
                                                                               length).ToLocalChecked()
                                                                         : v8::String::NewFromTwoByte(isolate,
                                                                                                      reinterpret_cast<const uint16_t *>(string_view.characters16()),
                                                                                                      v8::NewStringType::kNormal,
                                                                                                      length).ToLocalChecked();
            v8::String::Utf8Value utf8(isolate, string_view_str);

            std::string str = *utf8;
            return str;
        }

        class InspectorChannel final : public v8_inspector::V8Inspector::Channel {
        public:
            explicit InspectorChannel(v8::Local<v8::Context> context) {
                isolate_ = context->GetIsolate();
                context_.Reset(isolate_, context);
            }

            ~InspectorChannel() override = default;

        private:

            void sendResponse(
                    int callId,
                    std::unique_ptr<v8_inspector::StringBuffer> message) override {
                Send(message->string());
            }

            void sendNotification(
                    std::unique_ptr<v8_inspector::StringBuffer> message) override {
                Send(message->string());
            }

            void flushProtocolNotifications() override {}

            void Send(const v8_inspector::StringView &string) {
                // inspector protocol message can be send
                // to other service like
                // chrome inspector
                printf("InspectorChannel received.");
            }

            v8::Isolate *isolate_;
            v8::Global<v8::Context> context_;
        };

        class InspectorClient : public v8_inspector::V8InspectorClient {
        public:
            InspectorClient(v8::Local<v8::Context> context, bool connect) {
                if (!connect) return;
                isolate_ = context->GetIsolate();
                // initial inspector channel and session
                channel_.reset(new InspectorChannel(context));
                inspector_ = v8_inspector::V8Inspector::create(isolate_, this);
                session_ = inspector_->connect(kContextGroupId, channel_.get(), v8_inspector::StringView());
                // configure inspector context
                context->SetAlignedPointerInEmbedderData(kModuleEmbedderDataIndex, this);
                inspector_->contextCreated(v8_inspector::V8ContextInfo(
                        context, kContextGroupId, v8_inspector::StringView()
                ));

                // expose extras functions to context for inspector
                context_.Reset(isolate_, context);
            }

            void consoleAPIMessage(int contextGroupId, v8::Isolate::MessageErrorLevel level,
                                   const v8_inspector::StringView &message, const v8_inspector::StringView &url,
                                   unsigned int lineNumber, unsigned int columnNumber,
                                   v8_inspector::V8StackTrace *) override {
                int length = static_cast<int>(message.length());
                // todo: length check
                // DCHECK_LT(length, v8::String::kMaxLength);
                // parse StringView to string
                std::string message_str = StringViewToUtf8Value(isolate_, message);
                std::string url_str = StringViewToUtf8Value(isolate_, url);
                // better formatting for url

                char *tpl = "Log %*s > %s\n";
                url_str = url_str.substr(url_str.find_last_of("/\\") + 1) + ":" + std::to_string(lineNumber) + ":" +
                          std::to_string(columnNumber);
                if (level == v8::Isolate::MessageErrorLevel::kMessageError ||
                    level == v8::Isolate::MessageErrorLevel::kMessageWarning) {
                    fprintf(stderr, tpl,
                            20, url_str.c_str(), message_str.c_str()
                    );
                } else {
                    fprintf(stdout, tpl,
                           20, url_str.c_str(), message_str.c_str()
                    );
                }


            }

        private:

            static const int kContextGroupId = 1;
            std::unique_ptr<v8_inspector::V8Inspector> inspector_;
            std::unique_ptr<v8_inspector::V8InspectorSession> session_;
            std::unique_ptr<v8_inspector::V8Inspector::Channel> channel_;

            bool is_paused = false;
            v8::Global<v8::Context> context_;
            v8::Isolate *isolate_;
        };

    }
}


#endif //MAKI_W8_INSPECTOR_H
