(set-logic LIA)

(synth-fun combine ((lans Int) (rans Int) (lmin Int) (rmin Int)  (lmin2 Int) (rmin2 Int)) Int
    ((Start Int (lans
                 rans
                 lmin
                 rmin
                 lmin2
                 rmin2
                 0
                 1
                 (+ Start Start)
                 (- Start Start)
                 (ite StartBool Start Start)))
     (StartBool Bool ((and StartBool StartBool)
                      (or  StartBool StartBool)
                      (not StartBool)
                      (<=  Start Start)
                      (=   Start Start)
                      (>=  Start Start)))))


(define-fun max2 ((x Int) (y Int)) Int 
  (ite (>= x y) x y)
)
(define-fun ans ((lans Int) (rans Int) (lmin Int) (rmin Int)  (lmin2 Int) (rmin2 Int)) Int
  (ite (<= lans rmin)
       lans
       (ite (<= rans lmin)
            rans
            (max2 lmin2 rmin2))))


(declare-var a Int)
(declare-var b Int)
(declare-var c Int)
(declare-var d Int)
(declare-var e Int)
(declare-var f Int)
(constraint (= (ans a b c d e f) (combine a b c d e f)))

(check-synth)

(define-fun res ((Param0 Int) (Param1 Int) (Param2 Int) (Param3 Int) (Param4 Int) (Param5 Int)) Int (ite (<= Param0 Param3) Param0 (ite (<= Param1 Param2) Param1 (ite (<= Param5 Param4) Param4 Param5))))