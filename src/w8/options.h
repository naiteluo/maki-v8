//
// Created by Jiawei Tan on 2021/5/15.
//

#ifndef MAKI_OPTIONS_H
#define MAKI_OPTIONS_H

namespace w8 {
    class Options {
    public:

        char **argv;
        int argc;

        void Parse(int _argc, char **_argv) {
            argc = _argc;
            argv = _argv;
            for (int i = 0; i < argc; i++) {
                if (i == 0) {
                    base_path = argv[i];
                } else if (i >= 1) {
                    if (strstr(argv[i], ".js") != NULL || strstr(argv[i], ".mjs") != NULL) {
                        entry = argv[i];
                    } else {
                        if (strcmp(argv[i], "--inspector_enabled") == 0) {
                            inspector_enabled = true;
                        }
                    }
                }
            }
        }

        void Print() {
            printf("\n------\nargc = $i\n", argc);
            for (int i = 0; i < argc; i++) {
                printf("argv[%i] = %s\n", i, argv[i]);
            }
            printf("------\n\n");
        }

        template<class T>
        class DisallowReassignment {
        public:
            DisallowReassignment(const char *name, T value) : name_(name), value_(value) {}

            operator T() const { return value_; }

            T get() const { return value_; }

            DisallowReassignment &operator=(T value) {
                assert(!specified_);
                value_ = value;
                specified_ = true;
                return *this;
            }

            void Overwrite(T value) { value_ = value; }

        private:
            const char *name_;
            T value_;
            bool specified_ = false;
        };

        DisallowReassignment<const char *> base_path = {
                "base_path", NULL
        };

        DisallowReassignment<const char *> entry = {
                "entry_file", "w8.mjs"
        };

        DisallowReassignment<bool> inspector_enabled = {
                "inspector_enabled", false
        };
    };
}

#endif //MAKI_OPTIONS_H
