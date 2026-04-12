; ModuleID = 'sviroxk'
source_filename = "sviroxk"

@0 = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1
@1 = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1
@2 = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1

declare i32 @puts(ptr)

declare i32 @printf(ptr, ...)

define i32 @main() {
entry:
  %x = alloca i32, align 4
  store i32 5, ptr %x, align 4
  %p = alloca ptr, align 8
  store ptr %x, ptr %p, align 8
  %0 = load ptr, ptr %p, align 8
  %1 = load i32, ptr %0, align 4
  %2 = call i32 (ptr, ...) @printf(ptr @0, i32 %1)
  %3 = load ptr, ptr %p, align 8
  store i32 10, ptr %3, align 4
  %4 = load ptr, ptr %p, align 8
  %5 = load i32, ptr %4, align 4
  %6 = call i32 (ptr, ...) @printf(ptr @1, i32 %5)
  %7 = load i32, ptr %x, align 4
  %8 = call i32 (ptr, ...) @printf(ptr @2, i32 %7)
  ret i32 0
}
