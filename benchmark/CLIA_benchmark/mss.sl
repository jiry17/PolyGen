(set-logic LIA)

(synth-fun combine ((lmss Int) (rmss Int) (lmps Int) (rmps Int) (lmts Int) (rmts Int)) Int
    ((Start Int (lmss
                 rmss
                 lmps
                 rmps
                 lmts
                 rmts
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
(define-fun ans ((lmss Int) (rmss Int) (lmps Int) (rmps Int) (lmts Int) (rmts Int)) Int 
  (max2 (+ lmts rmps) (max2 lmss rmss)))


(declare-var a Int)
(declare-var b Int)
(declare-var c Int)
(declare-var d Int)
(declare-var e Int)
(declare-var f Int)
(constraint (= (ans a b c d e f) (combine a b c d e f)))

(check-synth)

