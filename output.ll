; ModuleID = 'sviroxk'
source_filename = "sviroxk"

@0 = private unnamed_addr constant [13 x i8] c"\D0\BF\D1\80\D0\B8\D0\B2\D0\B5\D1\82\00", align 1
@1 = private unnamed_addr constant [13 x i8] c"\D0\BF\D1\80\D0\B8\D0\B2\D0\B5\D1\82\00", align 1
@2 = private unnamed_addr constant [7 x i8] c"\D0\BC\D0\B8\D1\80\00", align 1
@3 = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1
@4 = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1
@5 = private unnamed_addr constant [3 x i8] c"%d\00", align 1

declare i32 @puts(ptr)

declare i32 @printf(ptr, ...)

define i32 @main() {
entry:
  %s1 = alloca ptr, align 8
  store ptr @0, ptr %s1, align 8
  %s2 = alloca ptr, align 8
  store ptr @1, ptr %s2, align 8
  %s3 = alloca ptr, align 8
  store ptr @2, ptr %s3, align 8
  %eq1 = alloca i32, align 4
  %0 = load ptr, ptr %s1, align 8
  %1 = load ptr, ptr %s2, align 8
  %2 = call i32 @strcmp(ptr %0, ptr %1)
  %3 = icmp eq i32 %2, 0
  store i1 %3, ptr %eq1, align 1
  %eq2 = alloca i32, align 4
  %4 = load ptr, ptr %s1, align 8
  %5 = load ptr, ptr %s3, align 8
  %6 = call i32 @strcmp(ptr %4, ptr %5)
  %7 = icmp eq i32 %6, 0
  store i1 %7, ptr %eq2, align 1
  %8 = load i32, ptr %eq1, align 4
  %9 = call i32 (ptr, ...) @printf(ptr @3, i32 %8)
  %10 = load i32, ptr %eq2, align 4
  %11 = call i32 (ptr, ...) @printf(ptr @4, i32 %10)
  %n = alloca i32, align 4
  store i32 42, ptr %n, align 4
  %ns = alloca ptr, align 8
  %strbuf = alloca [64 x i8], align 1
  %12 = getelementptr inbounds [64 x i8], ptr %strbuf, i32 0, i32 0
  %13 = load i32, ptr %n, align 4
  %14 = call i32 (ptr, ptr, ...) @sprintf(ptr %12, ptr @5, i32 %13)
  store ptr %12, ptr %ns, align 8
  %15 = load ptr, ptr %ns, align 8
  %16 = call i32 @puts(ptr %15)
  ret i32 0
}

declare i32 @strcmp(ptr, ptr)

declare i32 @sprintf(ptr, ptr, ...)
