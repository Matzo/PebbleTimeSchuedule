#import "PTSNotificationView.h"
@interface PTSNotificationView()
@property BOOL didDisplay;
@property (nonatomic, copy) void (^dismissAnimation)();
@end

@implementation PTSNotificationView

- (id)initWithDate:(NSDate*)date message:(NSString*)message {
    UIWindow *window = [[UIApplication sharedApplication] keyWindow];
    CGRect frame = CGRectMake(0, 0,
                              window.frame.size.width * 0.6,
                              80);
    frame.origin.x = (window.frame.size.width - frame.size.width) * 0.5;
    frame.origin.y = (window.frame.size.height - frame.size.height) * 0.5;
    
    self = [self initWithFrame:frame];
    if (self) {
        self.messageLabel.text = message;
        self.timeLabel.text = [date stringWithFormat:NSLocalizedString(@"FormatHourMinit", @"HH:mm")];
    }
    return self;
}

- (void)initialize {
    [super initialize];
    
    UIImage *image = [[UIImage imageNamed:@"event_frame"] resizableImage];
    UIImageView *frameImageView = [[UIImageView alloc] initWithImage:image];
    frameImageView.frame = self.bounds;
    [self addSubview:frameImageView];
    [self bringSubviewToFront:frameImageView];
}


- (void)show {
    self.didDisplay = NO;

    UIWindow *window = [[UIApplication sharedApplication] keyWindow];
    self.alpha = 0.0;
    self.center = window.center;
    [window addSubview:self];
    
    [UIView animateWithDuration:0.5 animations:^{
        self.alpha = 1.0;
    } completion:^(BOOL finished) {
        self.didDisplay = YES;
        if (finished) {
            if (self.dismissAnimation) {
                self.dismissAnimation();
            }
        }
    }];
}

- (void)dismissWithSuccess {
    __weak __typeof(self) weakSelf = self;
    self.dismissAnimation = ^{
        [UIView animateWithDuration:0.3 animations:^{
            CGRect frame = weakSelf.frame;
            frame.origin.y = - frame.size.height;
            weakSelf.frame = frame;
        } completion:^(BOOL finished) {
            [weakSelf removeFromSuperview];
            weakSelf.dismissAnimation = nil;
        }];
    };

    if (self.didDisplay) {
        self.dismissAnimation();
    }
}
- (void)dismissWithFailure {
    __weak __typeof(self) weakSelf = self;
    self.dismissAnimation = ^{
        [UIView animateWithDuration:0.3 animations:^{
            CGRect frame = weakSelf.frame;
            frame.origin.y += frame.size.height;
            weakSelf.frame = frame;
            weakSelf.alpha = 0.0;
        } completion:^(BOOL finished) {
            [weakSelf removeFromSuperview];
            weakSelf.dismissAnimation = nil;
        }];
    };

    if (self.didDisplay) {
        self.dismissAnimation();
    }
}

@end
