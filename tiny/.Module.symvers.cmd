cmd_/root/fly-tft/tiny/Module.symvers :=  sed 's/ko$$/o/'  /root/fly-tft/tiny/modules.order | scripts/mod/modpost -m -a -E   -o /root/fly-tft/tiny/Module.symvers -e -i Module.symvers -T - 
