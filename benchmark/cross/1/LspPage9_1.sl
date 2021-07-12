(set-logic LIA)

(synth-fun combine ((lsorted Int) (rsorted Int) (lhead Int) (rhead Int) (ltail Int) (rtail Int)) Int
    ((Start Int (lsorted
                 rsorted
                 lhead
                 rhead
                 ltail
                 rtail
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



(define-fun ans ((lsorted Int) (rsorted Int) (lhead Int) (rhead Int) (ltail Int) (rtail Int)) Int
  (ite (= 1 lsorted)
       (ite (= 1 rsorted)
            (ite (> ltail rhead)
                 0
                 1)
            0)
       0))

(define-fun ib ((x Int)) Bool (and (>= x 0) (<= x 1)))

(declare-var a Int)
(declare-var b Int)
(declare-var c Int)
(declare-var d Int)
(declare-var e Int)
(declare-var f Int)
(constraint (=> (and (ib a) (ib b)) (= (ans a b c d e f) (combine a b c d e f))))

(check-synth)

(define-fun res ((Param0 Int) (Param1 Int) (Param2 Int) (Param3 Int) (Param4 Int)) Int (ite (and (<= Param4 Param3) (<= Param0 Param1)) Param0 0))