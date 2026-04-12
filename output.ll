; ModuleID = 'sviroxk'
source_filename = "sviroxk"

@0 = private unnamed_addr constant [9 x i8] c"test.txt\00", align 1
@1 = private unnamed_addr constant [2 x i8] c"w\00", align 1
@2 = private unnamed_addr constant [26 x i8] c"\D0\BF\D1\80\D0\B8\D0\B2\D0\B5\D1\82 \D0\B8\D0\B7 SVIROXK\00", align 1
@3 = private unnamed_addr constant [24 x i8] c"\D1\84\D0\B0\D0\B9\D0\BB \D0\B7\D0\B0\D0\BF\D0\B8\D1\81\D0\B0\D0\BD\00", align 1

declare i32 @puts(ptr)

declare i32 @printf(ptr, ...)

define i32 @main() {
entry:
  %f = alloca ptr, align 8
  %0 = call ptr @fopen(ptr @0, ptr @1)
  store ptr %0, ptr %f, align 8
  %1 = load ptr, ptr %f, align 8
  %2 = call i32 @fputs(ptr @2, ptr %1)
  %3 = load ptr, ptr %f, align 8
  %4 = call i32 @fclose(ptr %3)
  %5 = call i32 @puts(ptr @3)
  ret i32 0
}

declare ptr @fopen(ptr, ptr)

declare i32 @fputs(ptr, ptr)

declare i32 @fclose(ptr)
