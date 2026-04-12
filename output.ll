; ModuleID = 'sviroxk'
source_filename = "sviroxk"

@0 = private unnamed_addr constant [26 x i8] c"\D0\BF\D1\80\D0\B8\D0\B2\D0\B5\D1\82 \D0\BE\D1\82 SVIROXK\00", align 1

define i32 @main() {
entry:
  %i = alloca i32, align 4
  store i32 0, ptr %i, align 4
  br label %loop

loop:                                             ; preds = %body, %entry
  %0 = load i32, ptr %i, align 4
  %1 = icmp slt i32 %0, 3
  br i1 %1, label %body, label %after

after:                                            ; preds = %loop
  ret i32 0

body:                                             ; preds = %loop
  %2 = call i32 @puts(ptr @0)
  %3 = add i32 %0, 1
  store i32 %3, ptr %i, align 4
  br label %loop
}

declare i32 @puts(ptr)

declare i32 @printf(ptr, ...)
