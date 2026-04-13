; ModuleID = 'sviroxk'
source_filename = "sviroxk"

@0 = private unnamed_addr constant [4 x i8] c"%f\0A\00", align 1
@1 = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1

declare i32 @puts(ptr)

declare i32 @printf(ptr, ...)

define i32 @main() {
entry:
  %x = alloca i32, align 4
  store i32 5, ptr %x, align 4
  %y = alloca double, align 8
  %0 = load i32, ptr %x, align 4
  %1 = sitofp i32 %0 to double
  store double %1, ptr %y, align 8
  %2 = load double, ptr %y, align 8
  %3 = call i32 (ptr, ...) @printf(ptr @0, double %2)
  %pi = alloca double, align 8
  store double 3.140000e+00, ptr %pi, align 8
  %z = alloca i32, align 4
  %4 = load double, ptr %pi, align 8
  %5 = fptosi double %4 to i32
  store i32 %5, ptr %z, align 4
  %6 = load i32, ptr %z, align 4
  %7 = call i32 (ptr, ...) @printf(ptr @1, i32 %6)
  ret i32 0
}
