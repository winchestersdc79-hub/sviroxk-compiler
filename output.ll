; ModuleID = 'sviroxk'
source_filename = "sviroxk"

@0 = private unnamed_addr constant [4 x i8] c"%c\0A\00", align 1

declare i32 @puts(ptr)

declare i32 @printf(ptr, ...)

define i32 @main() {
entry:
  %c = alloca i8, align 1
  store i32 65, ptr %c, align 4
  %0 = load i8, ptr %c, align 1
  %1 = call i32 (ptr, ...) @printf(ptr @0, i8 %0)
  ret i32 0
}
