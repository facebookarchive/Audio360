/*
 Copyright (c) 2018-present, Facebook, Inc.

 This source code is licensed under the MIT license found in the
 LICENSE file in the root directory of this source tree.
 */

#include "gtest/gtest.h"
#import <UIKit/UIKit.h>

int main(int argc, char *argv[])
{
   int returnValue = 0;

   try
   {
      testing::InitGoogleTest(&argc, argv);
      returnValue = RUN_ALL_TESTS();
   }
   catch (std::exception &exc)
   {
      returnValue = -1;
      std::cout << "Failed with exception " << exc.what() << "\n";
   }

   return returnValue;
}
