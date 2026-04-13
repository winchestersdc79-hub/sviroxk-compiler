; ModuleID = 'sviroxk'
source_filename = "sviroxk"

@0 = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1

declare i32 @puts(ptr)

declare i32 @printf(ptr, ...)

define i32 @main() {
entry:
  %i = alloca i32, align 4
  store i32 0, ptr %i, align 4
  br label %cond

cond:                                             ; preds = %merge, %then, %entry
  %0 = load i32, ptr %i, align 4
  %1 = icmp slt i32 %0, 10
  br i1 %1, label %body, label %after

body:                                             ; preds = %cond
  %2 = load i32, ptr %i, align 4
  %3 = add i32 %2, 1
  store i32 %3, ptr %i, align 4
  %4 = load i32, ptr %i, align 4
  %5 = icmp eq i32 %4, 5
  br i1 %5, label %then, label %else

after:                                            ; preds = %cond
  ret i32 0

then:                                             ; preds = %body
  br label %cond

else:                                             ; preds = %body
  br label %merge

merge:                                            ; preds = %else
  %6 = load i32, ptr %i, align 4
  %7 = call i32 (ptr, ...) @printf(ptr @0, i32 %6)
  br label %cond
}
