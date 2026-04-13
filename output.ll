; ModuleID = 'sviroxk'
source_filename = "sviroxk"

@0 = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1

declare i32 @puts(ptr)

declare i32 @printf(ptr, ...)

define i32 @square(i32 %x) {
entry:
  %x1 = alloca i32, align 4
  store i32 %x, ptr %x1, align 4
  %0 = load i32, ptr %x1, align 4
  %1 = load i32, ptr %x1, align 4
  %2 = mul i32 %0, %1
  ret i32 %2
}

define i32 @main() {
entry:
  %result = alloca i32, align 4
  %0 = call i32 @square(i32 5)
  store i32 %0, ptr %result, align 4
  %1 = load i32, ptr %result, align 4
  %2 = call i32 (ptr, ...) @printf(ptr @0, i32 %1)
  ret i32 0
}
