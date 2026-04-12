; ModuleID = 'sviroxk'
source_filename = "sviroxk"

@0 = private unnamed_addr constant [20 x i8] c"\D1\80\D0\B5\D0\B7\D1\83\D0\BB\D1\8C\D1\82\D0\B0\D1\82:\00", align 1
@1 = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1

define i32 @main() {
entry:
  %x = alloca i32, align 4
  store i32 8, ptr %x, align 4
  %0 = call i32 @puts(ptr @0)
  %1 = load i32, ptr %x, align 4
  %2 = call i32 (ptr, ...) @printf(ptr @1, i32 %1)
  ret i32 0
}

declare i32 @puts(ptr)

declare i32 @printf(ptr, ...)
