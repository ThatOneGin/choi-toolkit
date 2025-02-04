(defconst casm-mode-syntax-table
  (with-syntax-table (copy-syntax-table)
    (modify-syntax-entry ?/ ". 124b")
    (modify-syntax-entry ?* ". 23")
    (modify-syntax-entry ?\n "> b")
    (modify-syntax-entry ?' "\"")
    (syntax-table))
  "Syntax table for `choiasm-mode'")

(eval-and-compile
  (defconst casm-keys
    '("push" "mov" "add" "sub" "dump" "drop" "@proc" "@mem" "@push" "@pop" "syscall" "cmp" "jz" "jnz" "call" "jmp" "write" "ret" "halt")))

(defconst casm-h
  `((,(regexp-opt casm-keys 'symbols) . font-lock-keyword-face)
    ("@\\(mem\\|proc\\|push\\|pop\\)" . font-lock-keyword-face)))

(define-derived-mode choiasm-mode fundamental-mode "choi"
  "major mode for editing choi assembly"
  :syntax-table casm-mode-syntax-table
  (setq font-lock-defaults '(casm-h))
  (setq-local comment-start ";"))
(add-to-list 'auto-mode-alist '("\\.casm\\'" . choiasm-mode))
(provide 'choiasm-mode)
