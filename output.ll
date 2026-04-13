; ModuleID = 'sviroxk'
source_filename = "sviroxk"

@0 = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1

declare i32 @puts(ptr)

declare i32 @printf(ptr, ...)

define i32 @main() {
entry:
  %arr_0 = alloca i32, align 4
  store i32 1, ptr %arr_0, align 4
  %arr_1 = alloca i32, align 4
  store i32 2, ptr %arr_1, align 4
  %arr_2 = alloca i32, align 4
  store i32 3, ptr %arr_2, align 4
  store i32 10, ptr %arr_0, align 4
  %0 = load i32, ptr %arr_0, align 4
  %1 = call i32 (ptr, ...) @printf(ptr @0, i32 %0)
  ret i32 0
}
