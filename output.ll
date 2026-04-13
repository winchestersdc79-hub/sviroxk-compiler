; ModuleID = 'sviroxk'
source_filename = "sviroxk"

@0 = private unnamed_addr constant [4 x i8] c"%f\0A\00", align 1
@1 = private unnamed_addr constant [4 x i8] c"%f\0A\00", align 1
@2 = private unnamed_addr constant [4 x i8] c"%f\0A\00", align 1
@3 = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1

declare i32 @puts(ptr)

declare i32 @printf(ptr, ...)

define i32 @main() {
entry:
  %a = alloca double, align 8
  %0 = call double @floor(double 3.700000e+00)
  store double %0, ptr %a, align 8
  %b = alloca double, align 8
  %1 = call double @ceil(double 3.200000e+00)
  store double %1, ptr %b, align 8
  %c = alloca double, align 8
  %2 = call double @round(double 3.500000e+00)
  store double %2, ptr %c, align 8
  %d = alloca i32, align 4
  %3 = call i32 @rand()
  store i32 %3, ptr %d, align 4
  %4 = load double, ptr %a, align 8
  %5 = call i32 (ptr, ...) @printf(ptr @0, double %4)
  %6 = load double, ptr %b, align 8
  %7 = call i32 (ptr, ...) @printf(ptr @1, double %6)
  %8 = load double, ptr %c, align 8
  %9 = call i32 (ptr, ...) @printf(ptr @2, double %8)
  %10 = load i32, ptr %d, align 4
  %11 = call i32 (ptr, ...) @printf(ptr @3, i32 %10)
  ret i32 0
}

declare double @floor(double)

declare double @ceil(double)

declare double @round(double)

declare i32 @rand()
