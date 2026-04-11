; ModuleID = 'sviroxk'
source_filename = "sviroxk"

@0 = private unnamed_addr constant [20 x i8] c"\D0\BF\D1\80\D0\B8\D0\B2\D0\B5\D1\82 \D0\BC\D0\B8\D1\80\00", align 1

define i32 @main() {
entry:
  %0 = call i32 @puts(ptr @0)
  ret i32 0
}

declare i32 @puts(ptr)
