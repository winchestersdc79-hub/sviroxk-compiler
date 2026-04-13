; ModuleID = 'sviroxk'
source_filename = "sviroxk"

@0 = private unnamed_addr constant [4 x i8] c"%f\0A\00", align 1
@1 = private unnamed_addr constant [4 x i8] c"%f\0A\00", align 1
@2 = private unnamed_addr constant [4 x i8] c"%f\0A\00", align 1

declare i32 @puts(ptr)

declare i32 @printf(ptr, ...)

define i32 @main() {
entry:
  %x = alloca double, align 8
  store double 3.140000e+00, ptr %x, align 8
  %y = alloca double, align 8
  store double 2.000000e+00, ptr %y, align 8
  %z = alloca double, align 8
  %0 = load double, ptr %x, align 8
  %1 = load double, ptr %y, align 8
  %2 = fmul double %0, %1
  store double %2, ptr %z, align 8
  %3 = load double, ptr %x, align 8
  %4 = call i32 (ptr, ...) @printf(ptr @0, double %3)
  %5 = load double, ptr %y, align 8
  %6 = call i32 (ptr, ...) @printf(ptr @1, double %5)
  %7 = load double, ptr %z, align 8
  %8 = call i32 (ptr, ...) @printf(ptr @2, double %7)
  ret i32 0
}
