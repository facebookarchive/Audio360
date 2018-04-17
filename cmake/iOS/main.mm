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
