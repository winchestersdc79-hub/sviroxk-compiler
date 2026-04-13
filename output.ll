; ModuleID = 'sviroxk'
source_filename = "sviroxk"

@0 = private unnamed_addr constant [14 x i8] c"x \D1\8D\D1\82\D0\BE true\00", align 1
@1 = private unnamed_addr constant [15 x i8] c"x \D1\8D\D1\82\D0\BE false\00", align 1
@2 = private unnamed_addr constant [14 x i8] c"y \D1\8D\D1\82\D0\BE true\00", align 1
@3 = private unnamed_addr constant [15 x i8] c"y \D1\8D\D1\82\D0\BE false\00", align 1

declare i32 @puts(ptr)

declare i32 @printf(ptr, ...)

define i32 @main() {
entry:
  %x = alloca i32, align 4
  store i32 1, ptr %x, align 4
  %y = alloca i32, align 4
  store i32 0, ptr %y, align 4
  %0 = load i32, ptr %x, align 4
  %1 = icmp ne i32 %0, 0
  br i1 %1, label %then, label %else

then:                                             ; preds = %entry
  %2 = call i32 @puts(ptr @0)
  br label %merge

else:                                             ; preds = %entry
  %3 = call i32 @puts(ptr @1)
  br label %merge

merge:                                            ; preds = %else, %then
  %4 = load i32, ptr %y, align 4
  %5 = icmp ne i32 %4, 0
  br i1 %5, label %then1, label %else2

then1:                                            ; preds = %merge
  %6 = call i32 @puts(ptr @2)
  br label %merge3

else2:                                            ; preds = %merge
  %7 = call i32 @puts(ptr @3)
  br label %merge3

merge3:                                           ; preds = %else2, %then1
  ret i32 0
}
