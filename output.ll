; ModuleID = 'sviroxk'
source_filename = "sviroxk"

@0 = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1
@1 = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1
@2 = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1
@3 = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1
@4 = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1

declare i32 @puts(ptr)

declare i32 @printf(ptr, ...)

define i32 @main() {
entry:
  %a = alloca i32, align 4
  %0 = call double @sqrt(double 1.600000e+01)
  %1 = fptosi double %0 to i32
  store i32 %1, ptr %a, align 4
  %b = alloca i32, align 4
  store i32 5, ptr %b, align 4
  %c = alloca i32, align 4
  %2 = call double @pow(double 2.000000e+00, double 8.000000e+00)
  %3 = fptosi double %2 to i32
  store i32 %3, ptr %c, align 4
  %d = alloca i32, align 4
  store i32 7, ptr %d, align 4
  %e = alloca i32, align 4
  store i32 3, ptr %e, align 4
  %4 = load i32, ptr %a, align 4
  %5 = call i32 (ptr, ...) @printf(ptr @0, i32 %4)
  %6 = load i32, ptr %b, align 4
  %7 = call i32 (ptr, ...) @printf(ptr @1, i32 %6)
  %8 = load i32, ptr %c, align 4
  %9 = call i32 (ptr, ...) @printf(ptr @2, i32 %8)
  %10 = load i32, ptr %d, align 4
  %11 = call i32 (ptr, ...) @printf(ptr @3, i32 %10)
  %12 = load i32, ptr %e, align 4
  %13 = call i32 (ptr, ...) @printf(ptr @4, i32 %12)
  ret i32 0
}

declare double @sqrt(double)

declare double @pow(double, double)
