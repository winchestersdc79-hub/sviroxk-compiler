; ModuleID = 'sviroxk'
source_filename = "sviroxk"

@0 = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1
@1 = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1
@2 = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1

declare i32 @puts(ptr)

declare i32 @printf(ptr, ...)

define i32 @main() {
entry:
  %arr_0 = alloca i32, align 4
  store i32 10, ptr %arr_0, align 4
  %arr_1 = alloca i32, align 4
  store i32 20, ptr %arr_1, align 4
  %arr_2 = alloca i32, align 4
  store i32 30, ptr %arr_2, align 4
  %0 = load i32, ptr %arr_0, align 4
  %1 = call i32 (ptr, ...) @printf(ptr @0, i32 %0)
  %2 = load i32, ptr %arr_1, align 4
  %3 = call i32 (ptr, ...) @printf(ptr @1, i32 %2)
  %4 = load i32, ptr %arr_2, align 4
  %5 = call i32 (ptr, ...) @printf(ptr @2, i32 %4)
  ret i32 0
}
