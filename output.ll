; ModuleID = 'sviroxk'
source_filename = "sviroxk"

@0 = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1
@1 = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1
@2 = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1

declare i32 @puts(ptr)

declare i32 @printf(ptr, ...)

define i32 @main() {
entry:
  %arr = alloca ptr, align 8
  %0 = call ptr @malloc(i64 20)
  store ptr %0, ptr %arr, align 8
  %1 = load ptr, ptr %arr, align 8
  %2 = getelementptr i32, ptr %1, i32 0
  store i32 10, ptr %2, align 4
  %3 = load ptr, ptr %arr, align 8
  %4 = getelementptr i32, ptr %3, i32 1
  store i32 20, ptr %4, align 4
  %5 = load ptr, ptr %arr, align 8
  %6 = getelementptr i32, ptr %5, i32 2
  store i32 30, ptr %6, align 4
  %7 = load ptr, ptr %arr, align 8
  %8 = getelementptr i32, ptr %7, i32 0
  %9 = load i32, ptr %8, align 4
  %10 = call i32 (ptr, ...) @printf(ptr @0, i32 %9)
  %11 = load ptr, ptr %arr, align 8
  %12 = getelementptr i32, ptr %11, i32 1
  %13 = load i32, ptr %12, align 4
  %14 = call i32 (ptr, ...) @printf(ptr @1, i32 %13)
  %15 = load ptr, ptr %arr, align 8
  %16 = getelementptr i32, ptr %15, i32 2
  %17 = load i32, ptr %16, align 4
  %18 = call i32 (ptr, ...) @printf(ptr @2, i32 %17)
  %19 = load ptr, ptr %arr, align 8
  call void @free(ptr %19)
  ret i32 0
}

declare ptr @malloc(i64)

declare void @free(ptr)
