(set-logic LIA)

(synth-fun combine ((lmts Int) (rmts Int) (lsum Int) (rsum Int)) Int
    ((Start Int (lmts
                 rmts
                 lsum
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


(define-fun max2 ((x Int) (y Int)) Int 
  (ite (>= x y) x y)
)
(define-fun ans  ((lmts Int) (rmts Int) (lsum Int) (rsum Int)) Int
  (max2 rmts (+ rsum lmts)))


(declare-var a Int)
(declare-var b Int)
(declare-var c Int)
(declare-var d Int)
(constraint (= (ans a b c d) (combine a b c d)))

(check-synth)

