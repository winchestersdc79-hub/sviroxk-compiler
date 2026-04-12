; ModuleID = 'sviroxk'
source_filename = "sviroxk"

@0 = private unnamed_addr constant [6 x i8] c"hello\00", align 1
@1 = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1

declare i32 @puts(ptr)

declare i32 @printf(ptr, ...)

define i32 @main() {
entry:
  %s = alloca ptr, align 8
  store ptr @0, ptr %s, align 8
  %l = alloca i32, align 4
  %0 = load ptr, ptr %s, align 8
  %1 = call i32 @strlen(ptr %0)
  store i32 %1, ptr %l, align 4
  %2 = load i32, ptr %l, align 4
  %3 = call i32 (ptr, ...) @printf(ptr @1, i32 %2)
  ret i32 0
}

declare i32 @strlen(ptr)
