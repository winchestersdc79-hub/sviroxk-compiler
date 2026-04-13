; ModuleID = 'sviroxk'
source_filename = "sviroxk"

@0 = private unnamed_addr constant [6 x i8] c"%lld\0A\00", align 1

declare i32 @puts(ptr)

declare i32 @printf(ptr, ...)

define i32 @main() {
entry:
  %x = alloca i64, align 8
  store i64 9999999999, ptr %x, align 4
  %0 = load i64, ptr %x, align 4
  %1 = call i32 (ptr, ...) @printf(ptr @0, i64 %0)
  ret i32 0
}
