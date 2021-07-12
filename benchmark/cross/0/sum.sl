(set-logic LIA)

(synth-fun combine ((lsum Int) (rsum Int)) Int
    ((Start Int (lsum
                 rsum
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



(define-fun ans ((lsum Int) (rsum Int)) Int
   (+ lsum rsum))


(declare-var a Int)
(declare-var b Int)
(constraint (= (ans a b) (combine a b)))

(check-synth)

(define-fun res ((Param0 Int) (Param1 Int)) Int (+ Param0 Param1))