//
//  ViewController.m
//
//  Created by richardwallis on 15/11/2022.
//

#import "ViewController.h"
#import "EmulationController.h"

@interface ViewController ()

@property (nonatomic,readwrite) EmulationController*        systemController;
@property (nonatomic,readwrite) EmulationMetalView*         metalView;

@end

@implementation ViewController

- (void)viewDidLoad
{
    [super viewDidLoad];

    self.metalView = [EmulationMetalView new];
}

- (void) viewWillAppear
{
    if(self.systemController == nil)
    {
        [self.view addSubview:self.metalView];
        self.metalView.frame = self.view.frame;
        
        self.systemController = [[EmulationController alloc] initWithView:self.metalView];
        self.metalView.delegate = self.systemController;
    }
}

- (void) viewWillLayout
{
    self.metalView.frame = self.view.frame;
}

@end
