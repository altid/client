// Objective-C API for talking to github.com/altid/client Go package.
//   gobind -lang=objc github.com/altid/client
//
// File is generated by gobind. Do not edit.

#ifndef __Client_H__
#define __Client_H__

@import Foundation;
#include "ref.h"
#include "Universe.objc.h"


@class ClientClient;
@class ClientFeedIterator;

/**
 * Client represents a 9p client session
 */
@interface ClientClient : NSObject <goSeqRefInterface> {
}
@property(strong, readonly) _Nonnull id _ref;

- (nonnull instancetype)initWithRef:(_Nonnull id)ref;
/**
 * NewClient returns a client ready to connect to addr
 */
- (nullable instancetype)init:(NSString* _Nullable)addr;
/**
 * Aside returns the contents of the `aside` file for a given buffer
 */
- (NSData* _Nullable)aside:(NSError* _Nullable* _Nullable)error;
/**
 * Attach is called after optionally calling Auth
 */
- (BOOL)attach:(NSError* _Nullable* _Nullable)error;
/**
 * Auth is optionally called after Connect to authenticate with the server
 */
- (BOOL)auth:(NSError* _Nullable* _Nullable)error;
/**
 * Buffer changes the active buffer to the named buffer, or returns an error
 */
- (BOOL)buffer:(NSString* _Nullable)name ret0_:(long* _Nullable)ret0_ error:(NSError* _Nullable* _Nullable)error;
/**
 * Cleanup closes the underlying connection
 */
- (void)cleanup;
/**
 * Close attempts to close the named buffer
 */
- (BOOL)close:(NSString* _Nullable)name ret0_:(long* _Nullable)ret0_ error:(NSError* _Nullable* _Nullable)error;
// skipped method Client.Command with unsupported parameter or return types

/**
 * Connect performs the network dial for the connection
 */
- (BOOL)connect:(long)debug error:(NSError* _Nullable* _Nullable)error;
/**
 * Ctl returns the contents of the `ctl` file for a given service
 */
- (NSData* _Nullable)ctl:(NSError* _Nullable* _Nullable)error;
/**
 * Document returns the contents of a document file on the host
if it exists, or an error
 */
- (NSData* _Nullable)document:(NSError* _Nullable* _Nullable)error;
// skipped method Client.Feed with unsupported parameter or return types

/**
 * FeedIterator returns a new FeedIterator ready to go
 */
- (ClientFeedIterator* _Nullable)feedIterator:(NSError* _Nullable* _Nullable)error;
// skipped method Client.GetCommands with unsupported parameter or return types

/**
 * Input appends the given data string to input
 */
- (BOOL)input:(NSData* _Nullable)data ret0_:(long* _Nullable)ret0_ error:(NSError* _Nullable* _Nullable)error;
/**
 * Link updates the current buffer to the named buffer, closing the former
 */
- (BOOL)link:(NSString* _Nullable)name ret0_:(long* _Nullable)ret0_ error:(NSError* _Nullable* _Nullable)error;
/**
 * Notifications returns and clears any pending notifications
 */
- (NSData* _Nullable)notifications:(NSError* _Nullable* _Nullable)error;
/**
 * Open attempts to open the named buffer
 */
- (BOOL)open:(NSString* _Nullable)name ret0_:(long* _Nullable)ret0_ error:(NSError* _Nullable* _Nullable)error;
// skipped method Client.Send with unsupported parameter or return types

/**
 * Status returns the contents of the `status` file for a given buffer
 */
- (NSData* _Nullable)status:(NSError* _Nullable* _Nullable)error;
/**
 * Tabs returns the contents of the `tabs` file for the server
 */
- (NSData* _Nullable)tabs:(NSError* _Nullable* _Nullable)error;
/**
 * Title returns the contents of the `title` file for a given buffer
 */
- (NSData* _Nullable)title:(NSError* _Nullable* _Nullable)error;
@end

/**
 * FeedIterator allows you to step through lines of feed with Next()
Useful for gomobile, etc
 */
@interface ClientFeedIterator : NSObject <goSeqRefInterface> {
}
@property(strong, readonly) _Nonnull id _ref;

- (nonnull instancetype)initWithRef:(_Nonnull id)ref;
- (nonnull instancetype)init;
/**
 * Next will return the next slice of bytes, or an error
After an error, future calls to Next() will panic
 */
- (NSData* _Nullable)next:(NSError* _Nullable* _Nullable)error;
@end

/**
 * MSIZE - maximum size for a message
 */
FOUNDATION_EXPORT const int64_t ClientMSIZE;

/**
 * NewClient returns a client ready to connect to addr
 */
FOUNDATION_EXPORT ClientClient* _Nullable ClientNewClient(NSString* _Nullable addr);

/**
 * NewMockClient returns a client for testing
Feed, if called, will be populated with data from google's GoFuzz every 100ms
 */
FOUNDATION_EXPORT ClientClient* _Nullable ClientNewMockClient(NSString* _Nullable addr);

#endif
