; ModuleID = 'sviroxk'
source_filename = "sviroxk"

@0 = private unnamed_addr constant [20 x i8] c"\D0\BF\D1\80\D0\B8\D0\B2\D0\B5\D1\82 \D0\BC\D0\B8\D1\80\00", align 1
@1 = private unnamed_addr constant [34 x i8] c"\D1\8F\D0\B7\D1\8B\D0\BA SVIROXK \D1\80\D0\B0\D0\B1\D0\BE\D1\82\D0\B0\D0\B5\D1\82\00", align 1

define i32 @main() {
entry:
  %0 = call i32 @puts(ptr @0)
  %1 = call i32 @puts(ptr @1)
  ret i32 0
}

declare i32 @puts(ptr)
