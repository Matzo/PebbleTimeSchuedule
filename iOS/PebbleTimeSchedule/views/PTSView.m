#import "PTSView.h"

@implementation PTSView

- (id)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        // Initialization code
        [self initialize];
    }
    return self;
}

- (void)awakeFromNib {
    [self initialize];
}

- (void)initialize {
    UINib *nib = [UINib nibWithNibName:NSStringFromClass([self class]) bundle:[NSBundle bundleForClass:([self class])]];
    NSArray *views = [nib instantiateWithOwner:self options:nil];
    UIView *firstView = [views firstObject];
    
    firstView.autoresizingMask = (UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight);
    self.backgroundColor = firstView.backgroundColor;

    if (self.bounds.size.width == 0 && self.bounds.size.height == 0) {
        self.bounds = firstView.bounds;
    } else {
        firstView.frame = self.bounds;
    }
    [self addSubview:firstView];
}

@end
