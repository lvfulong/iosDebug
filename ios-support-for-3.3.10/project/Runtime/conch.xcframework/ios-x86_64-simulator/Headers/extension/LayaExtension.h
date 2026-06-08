/**
 * LayaExtension.h - LayaNative Extension Plugin System Public C API
 *
 * This is the stable ABI contract for extension authors.
 * Extensions include ONLY this header. All types are pure C for ABI stability.
 *
 * Extensions use jsvm API directly for JS interop (jsvm_env, jsvm_value, etc.).
 * Include <jsvm/JSVM.h> for the full jsvm API.
 *
 * Supported platforms: Windows (.dll), Android (.so), iOS (.a static),
 *                      Linux (.so), OHOS (.so)
 */
#pragma once

#include <stdint.h>
#include <stddef.h>
#include <jsvm/JSVM.h>

/* -------------------------------------------------------------------------- */
/*  Platform export macro                                                     */
/* -------------------------------------------------------------------------- */
#if defined(_WIN32)
    #define LAYA_EXT_EXPORT __declspec(dllexport)
#elif defined(__GNUC__) || defined(__clang__)
    #define LAYA_EXT_EXPORT __attribute__((visibility("default")))
#else
    #define LAYA_EXT_EXPORT
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* -------------------------------------------------------------------------- */
/*  Version                                                                   */
/* -------------------------------------------------------------------------- */
#define LAYA_EXTENSION_API_VERSION 1

/* -------------------------------------------------------------------------- */
/*  Lifecycle event types                                                     */
/* -------------------------------------------------------------------------- */
typedef enum {
    LAYA_EXT_EVENT_INIT    = 0, /** Called during extension loading (register JS classes here) */
    LAYA_EXT_EVENT_DEINIT  = 1, /** Called during extension unloading */
} LayaExtEventType;

/* -------------------------------------------------------------------------- */
/*  Callback signatures                                                       */
/* -------------------------------------------------------------------------- */

/** Frame callback. Called each frame if registered. */
typedef void (*LayaExtFrameCallback)(float dt, void* user_data);

/** Generic callback for post_to_js. */
typedef void (*LayaExtVoidCallback)(void* user_data);

/* -------------------------------------------------------------------------- */
/*  Engine-provided interface (function pointer table)                        */
/* -------------------------------------------------------------------------- */

/**
 * LayaExtensionInterface - The engine passes this struct to extensions.
 * Extensions call these function pointers to interact with the engine.
 *
 * For JS interop (creating values, calling functions, etc.), use the jsvm API
 * directly (jsvm_create_int32, jsvm_get_value_double, jsvm_wrap, etc.).
 * The jsvm_env can be obtained via get_env().
 *
 * All functions are safe to call only from the JS thread unless noted.
 */
typedef struct {
    uint32_t api_version;

    /* --- JS Environment --- */

    /** Get the current jsvm_env. Only valid on the JS thread. */
    jsvm_env (*get_env)();

    /**
     * Get the exports object for this extension.
     * The engine creates an empty JS object and mounts it on the global scope
     * as global[extensionName]. Extensions should register their functions and
     * classes on this object instead of directly on the global object.
     *
     * JS usage: extensionName.myFunction(), new extensionName.MyClass()
     *
     * Only valid during on_event(INIT) and after.
     */
    jsvm_value (*get_exports)();

    /* --- Threading --- */

    /**
     * Post a callback to the JS thread. Safe to call from any thread.
     * The callback will be invoked on the JS thread's message loop.
     */
    void (*post_to_js)(LayaExtVoidCallback callback, void* user_data);

    /* --- Logging --- */

    void (*log_info)(const char* msg);
    void (*log_warn)(const char* msg);
    void (*log_error)(const char* msg);

} LayaExtensionInterface;

/* -------------------------------------------------------------------------- */
/*  Extension initialization info (filled by extension entry point)           */
/* -------------------------------------------------------------------------- */

/**
 * The extension fills this struct in its entry point to describe itself
 * and provide lifecycle callbacks.
 */
typedef struct {
    /** Must be set to LAYA_EXTENSION_API_VERSION */
    uint32_t api_version;
    /** Extension name (static string, must outlive the extension) */
    const char* name;
    /** Extension version string */
    const char* version;
    /**
     * Lifecycle event callback. Called for INIT, DEINIT, PAUSE, RESUME.
     * @return 0 on success, non-zero on failure
     */
    int (*on_event)(LayaExtEventType event, const LayaExtensionInterface* iface, void* user_data);
    /** User data passed to on_event */
    void* user_data;
} LayaExtensionInitInfo;

/* -------------------------------------------------------------------------- */
/*  Entry point                                                               */
/* -------------------------------------------------------------------------- */

/**
 * Entry point function signature.
 * The engine calls this after loading the extension library.
 * @param engine_interface  Engine function table (valid for lifetime of extension)
 * @param out_info          Extension fills this with its info and callbacks
 * @return 0 on success, non-zero on failure
 */
typedef int (*LayaExtensionEntryFunc)(
    const LayaExtensionInterface* engine_interface,
    LayaExtensionInitInfo* out_info);

/** The symbol name that the engine looks for via dlsym/GetProcAddress */
#define LAYA_EXTENSION_ENTRY_SYMBOL "laya_extension_init"

/**
 * Convenience macro for defining the extension entry point.
 * Exports the generic symbol "laya_extension_init" for dynamic loading.
 * Usage:
 *   static int my_init(const LayaExtensionInterface* e, LayaExtensionInitInfo* i) { ... }
 *   LAYA_EXTENSION_ENTRY(my_init)
 */
#ifdef __cplusplus
#define LAYA_EXTENSION_ENTRY(init_func)                                         \
    extern "C" LAYA_EXT_EXPORT int laya_extension_init(                         \
        const LayaExtensionInterface* engine_interface,                          \
        LayaExtensionInitInfo* out_info) {                                       \
        return init_func(engine_interface, out_info);                            \
    }
#else
#define LAYA_EXTENSION_ENTRY(init_func)                                         \
    LAYA_EXT_EXPORT int laya_extension_init(                                    \
        const LayaExtensionInterface* engine_interface,                          \
        LayaExtensionInitInfo* out_info) {                                       \
        return init_func(engine_interface, out_info);                            \
    }
#endif

/**
 * Named entry point macro for iOS static linking (Godot GDExtension 方式).
 *
 * 导出带扩展名的符号 "laya_extension_init_<ext_name>"，用 __attribute__((used))
 * 防止编译器剥离。iOS 上扩展静态库需合并进 conch.xcframework（由 build.sh
 * 自动完成），引擎通过 manifest 中的扩展名构造符号名，用 dlsym(RTLD_SELF) 查找。
 *
 * Usage:
 *   static int my_init(const LayaExtensionInterface* e, LayaExtensionInitInfo* i) { ... }
 *   LAYA_EXTENSION_ENTRY(my_init)
 *   LAYA_EXTENSION_ENTRY_NAMED(my_extension, my_init)
 */
#if defined(__GNUC__) || defined(__clang__)
#define _LAYA_EXT_USED __attribute__((used))
#else
#define _LAYA_EXT_USED
#endif

#ifdef __cplusplus
#define LAYA_EXTENSION_ENTRY_NAMED(ext_name, init_func)                         \
    extern "C" LAYA_EXT_EXPORT _LAYA_EXT_USED                                  \
    int laya_extension_init_##ext_name(                                        \
        const LayaExtensionInterface* engine_interface,                          \
        LayaExtensionInitInfo* out_info) {                                       \
        return init_func(engine_interface, out_info);                            \
    }
#else
#define LAYA_EXTENSION_ENTRY_NAMED(ext_name, init_func)                         \
    LAYA_EXT_EXPORT _LAYA_EXT_USED                                              \
    int laya_extension_init_##ext_name(                                        \
        const LayaExtensionInterface* engine_interface,                          \
        LayaExtensionInitInfo* out_info) {                                       \
        return init_func(engine_interface, out_info);                            \
    }
#endif

#ifdef __cplusplus
}
#endif
