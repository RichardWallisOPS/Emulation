//
//  ViewController.m
//  MetalTest
//
//  Created by richardwallis on 15/11/2016.
//  Copyright © 2016 openplanetsoftware. All rights reserved.
//

#import "ViewController.h"
#import "MetalRenderer.h"

@interface ViewController ()

@property (nonatomic,readwrite) MetalRenderer*  renderer;
@property (nonatomic,readwrite) MTKView*        metalView;

@end

@implementation ViewController

- (void)viewDidLoad
{
    [super viewDidLoad];

    self.metalView = [MTKView new];
}

- (void) viewWillAppear
{
    if(self.renderer == nil)
    {
        self.view.frame = CGRectMake(0, 0, 1024, 960);
        [self.view addSubview:self.metalView];
        self.metalView.frame = self.view.frame;
        
        NSProcessInfo* process = [NSProcessInfo processInfo];
        NSArray* arguments = [process arguments];
        
        NSString* path = arguments[1];
        
        NSURL* gamePakURL = [[NSURL alloc] initFileURLWithPath:path isDirectory:NO];
        
        NSError* pError = nil;
        NSData* pakData = [NSData dataWithContentsOfURL:gamePakURL options:0 error:&pError];

        self.renderer = [[MetalRenderer alloc] initWithView:self.metalView gamePakData:pakData];
        self.metalView.delegate = self.renderer;
        
#if DEBUG
        NSAssert(self.renderer != nil, @"Can't create Metal Renderer");
#endif
    }
}

- (void) viewWillLayout
{
    self.metalView.frame = self.view.frame;
}

@end
