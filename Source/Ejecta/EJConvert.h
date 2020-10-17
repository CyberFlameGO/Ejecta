// EJConvert.h provides various functions to convert native values and objects
// to and from JavaScript values and objects. These functions are used
// throughout all of Ejecta.

// JSValueToNSString() and NSStringToJSValue() convert native Obj-C to and from
// JSValueRefs.

// JSValueToNumberFast() provides a faster implementation of JSCs built-in
// JSValueToNumber() function at expense of no error checking - i.e. strings
// will silently convert to 0 instead of throwing an exception. Ejecta makes
// heavy use of this function to provide a fast(er) Canvas and WebGL Context.

// NSObjectToJSValue() and JSValueToNSObject() converts between complex object
// hierachies. It supports Arrays, Objects (as NSDictionary), Strings, Bools,
// Numbers and Date instances.

// JSValueGetTypedArrayPtr() gets the data pointer and byte length from a
// Typed Array or Array Buffer. 

#import <Foundation/Foundation.h>
#import "JavaScriptCore/JavaScriptCore.h"

// JSValueToNumberFast can skip locking the whole JSContext if we know the
// layout of JSValues in memory. Sadly, the JSC public API does not expose
// such a function itself and the memory layout _has_ changed with iOS14
// See here for the details:
// https://github.com/WebKit/webkit/commit/e522feb5a8bfe5ba78fd1749df342f56dc98fcb5
// So now we default to not use the fast, private API anymore and instead
// do the number conversion by the book :/
#define EJ_JSC_USE_FAST_PRIVATE_API false

NSString *JSValueToNSString( JSContextRef ctx, JSValueRef v );
JSValueRef NSStringToJSValue( JSContextRef ctx, NSString *string );
double JSValueToNumberFast( JSContextRef ctx, JSValueRef v );
void JSValueUnprotectSafe( JSContextRef ctx, JSValueRef v );
JSValueRef NSObjectToJSValue( JSContextRef ctx, NSObject *obj );
NSObject *JSValueToNSObject( JSContextRef ctx, JSValueRef value );
void *JSValueGetTypedArrayPtr( JSContextRef ctx, JSValueRef value, size_t *length );

static inline void *JSValueGetPrivate(JSValueRef v) {
	// On 64bit systems we can not safely call JSObjectGetPrivate with any
	// JSValueRef. Doing so with immediate values (numbers, null, bool,
	// undefined) will crash the app. So we check for these first.

	#if __LP64__
		return !((int64_t)v & 0xfffe000000000002ll)
			? JSObjectGetPrivate((JSObjectRef)v)
			: NULL;
	#else
		return JSObjectGetPrivate((JSObjectRef)v);
	#endif
}
