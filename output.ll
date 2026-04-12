; ModuleID = 'sviroxk'
source_filename = "sviroxk"

@0 = private unnamed_addr constant [24 x i8] c"\D0\B2\D0\B2\D0\B5\D0\B4\D0\B8 \D1\87\D0\B8\D1\81\D0\BB\D0\BE: \00", align 1
@1 = private unnamed_addr constant [3 x i8] c"%d\00", align 1
@2 = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1

declare i32 @puts(ptr)

declare i32 @printf(ptr, ...)

define i32 @main() {
entry:
  %x = alloca i32, align 4
  %0 = call i32 (ptr, ...) @printf(ptr @0)
  %input_tmp = alloca i32, align 4
  %1 = call i32 (ptr, ...) @scanf(ptr @1, ptr %input_tmp)
  %2 = load i32, ptr %input_tmp, align 4
  store i32 %2, ptr %x, align 4
  %3 = load i32, ptr %x, align 4
  %4 = call i32 (ptr, ...) @printf(ptr @2, i32 %3)
  ret i32 0
}

declare i32 @scanf(ptr, ...)
