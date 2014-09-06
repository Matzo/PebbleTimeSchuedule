#import <UIKit/UIKit.h>

@interface UIImage (PTSFoundation)
+ (UIImage *)imageWithSize:(CGSize)size opaque:(BOOL)opaque scale:(CGFloat)scale render:(void (^)(CGContextRef c))render;

- (UIImage*)resizableImage;
- (UIImage *)darkenImage;
@end
