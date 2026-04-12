; ModuleID = 'sviroxk'
source_filename = "sviroxk"

@0 = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1

define i32 @main() {
entry:
  %x = alloca i32, align 4
  store i32 0, ptr %x, align 4
  br label %cond

cond:                                             ; preds = %body, %entry
  %0 = load i32, ptr %x, align 4
  %1 = icmp slt i32 %0, 3
  br i1 %1, label %body, label %after

body:                                             ; preds = %cond
  %2 = load i32, ptr %x, align 4
  %3 = call i32 (ptr, ...) @printf(ptr @0, i32 %2)
  %4 = load i32, ptr %x, align 4
  %5 = add i32 %4, 1
  store i32 %5, ptr %x, align 4
  br label %cond

after:                                            ; preds = %cond
  ret i32 0
}

declare i32 @puts(ptr)

declare i32 @printf(ptr, ...)
