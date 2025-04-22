(setq arduino-cli-path "D:/arduino/arduino-cli_1.2.0_Windows_64bit/arduino-cli.exe")
(setq arduino-project-path "D:/arduino/arduino")

;; Install and configure straight.el (bootstrap)
(defvar bootstrap-version)
(let ((bootstrap-file
       (expand-file-name "straight/repos/straight.el/bootstrap.el" user-emacs-directory))
      (bootstrap-version 6))
  (unless (file-exists-p bootstrap-file)
    (with-current-buffer
        (url-retrieve-synchronously
         "https://raw.githubusercontent.com/radian-software/straight.el/develop/install.el"
         'silent 'inhibit-cookies)
      (goto-char (point-max))
      (eval-print-last-sexp)))
  (load bootstrap-file nil 'nomessage))

(setq straight-use-package-by-default t)

;; UI tweaks
(setq inhibit-startup-screen t
      initial-scratch-message nil
      make-backup-files nil
      ring-bell-function 'ignore
      cursor-type 'bar
      compilation-directory-locked nil
      enable-local-variables t)

(menu-bar-mode -1)
(tool-bar-mode -1)
(scroll-bar-mode -1)

;; Baþlangýçta tam ekran ve ortadan ikiye böl
(add-to-list 'default-frame-alist '(fullscreen . fullboth))
(add-hook 'emacs-startup-hook
          (lambda ()
            (delete-other-windows)
            (split-window-right)))

;; Geliþtirilmiþ görünüm
(global-hl-line-mode 1)
(set-face-background 'hl-line "midnight blue")
(global-display-line-numbers-mode 1)
(show-paren-mode 1)

;; Sekme ve girinti ayarlarý
(setq-default tab-width 4
              indent-tabs-mode nil)

;; Yazý tipi
(set-frame-font "Fira Code Retina-9" nil t)

;; Kaydýrma
(setq scroll-conservatively 101
      mouse-wheel-scroll-amount '(1 ((shift) . 1)))

;; Geri alma sýnýrlarý
(setq undo-limit 20000000)
(setq undo-strong-limit 40000000)

;; Otomatik eþleþen parantez ve dosya izleme
(electric-pair-mode 1)
(global-auto-revert-mode 1)

;; Daha iyi buffer listesi
(global-set-key (kbd "C-x C-b") 'ibuffer)

;; Company setup
(straight-use-package 'company)
(require 'company)
(global-company-mode 1)

;; Arduino CLI fonksiyonlarý
(defun arduino-compile ()
  "Compile Arduino project using arduino-cli."
  (interactive)
  (setq compilation-read-command nil)
  (compile (format "%s compile --fqbn arduino:avr:mega %s"
                   arduino-cli-path arduino-project-path)))

(defun arduino-upload ()
  "Upload compiled code to Arduino Mega 2560."
  (interactive)
  (compile (format "%s upload -p COM5 --fqbn arduino:avr:mega %s"
                   arduino-cli-path arduino-project-path)))

(defun arduino-serial-monitor ()
  "Open serial monitor at 115200 baud rate."
  (interactive)
  (compile (format "%s monitor -p COM5 -c baudrate=115200"
                   arduino-cli-path)))
;; -----------------------------
;; 1) Paket sistemi ve MELPA
;; -----------------------------
(require 'package)
(setq package-archives
      '(("melpa" . "https://melpa.org/packages/")
        ("gnu"   . "https://elpa.gnu.org/packages/")))
(package-initialize)
(unless package-archive-contents
  (package-refresh-contents))
(unless (package-installed-p 'dracula-theme)
  (package-install 'dracula-theme))

;; -----------------------------
;; 2) custom-file (varsa)
;; -----------------------------
(setq custom-file (expand-file-name "custom.el" user-emacs-directory))
(when (file-exists-p custom-file)
  (load custom-file t))

;; -----------------------------
;; 3) UTF-8 kodlama ayarlarý
;; -----------------------------
(set-language-environment "UTF-8")
(prefer-coding-system       'utf-8)
(setq coding-system-for-read  'utf-8
      coding-system-for-write 'utf-8
      default-buffer-file-coding-system 'utf-8-unix)

;; -----------------------------
;; 4) Otomatik tema yükleme
;; -----------------------------
(when (boundp 'custom-enabled-themes)
  (mapc #'disable-theme custom-enabled-themes))
(load-theme 'dracula t)
