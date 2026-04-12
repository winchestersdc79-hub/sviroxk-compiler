; ModuleID = 'sviroxk'
source_filename = "sviroxk"

@0 = private unnamed_addr constant [17 x i8] c"x \D0\B1\D0\BE\D0\BB\D1\8C\D1\88\D0\B5 5\00", align 1
@1 = private unnamed_addr constant [17 x i8] c"x \D0\BC\D0\B5\D0\BD\D1\8C\D1\88\D0\B5 5\00", align 1

define i32 @main() {
entry:
  %x = alloca i32, align 4
  store i32 10, ptr %x, align 4
  %0 = load i32, ptr %x, align 4
  %1 = icmp sgt i32 %0, 5
  br i1 %1, label %then, label %else

then:                                             ; preds = %entry
  %2 = call i32 @puts(ptr @0)
  br label %merge

else:                                             ; preds = %entry
  %3 = call i32 @puts(ptr @1)
  br label %merge

merge:                                            ; preds = %else, %then
  ret i32 0
}

declare i32 @puts(ptr)

declare i32 @printf(ptr, ...)
