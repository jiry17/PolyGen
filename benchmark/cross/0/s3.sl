; if x==y then f(x,y)=x+y
; else if x>y then f(x,y)=1
; else f(x,y)=-1

(set-logic LIA)

(synth-fun f ((x Int) (y Int)) Int
   ((Start Int (x
                y
                0 1 -1 2 -2
                 (+ Start Start)
                 (- Start Start)
                 (ite StartBool Start Start)))
     (StartBool Bool ((and StartBool StartBool)
                      (or  StartBool StartBool)
                      (not StartBool)
                      (<=  Start Start)
                      (=   Start Start)
                      (>=  Start Start)))))

(declare-var x Int)
(declare-var y Int)

(constraint (=> (= x y) (= (f x y) (+ x y))))
(constraint (=> (> x y) (= (f x y) 1)))
(constraint (=> (< x y) (= (f x y) -1)))

(check-synth)

(define-fun res ((Param0 Int) (Param1 Int)) Int (ite (and (<= Param1 Param0) (<= Param0 Param1)) (* Param0 2) (ite (<= Param1 Param0) 1 -1)))