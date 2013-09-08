control system web interface 
----------------------------

install
-------
web server : apt-get install boa . Problem /etc/hosts där fanns inte Irritator med -> boa kunde inte starta

docs for perl: apt-get install perl-doc

perl modules: perl -MCPAN -e 'install Convert::Binary::C'





files at /var/www

- index.html
- nial.jpg


files at /usr/lib/cgi-bin

- index.cgi               # main script
- commands.cgi            # stop hvps, init_hw, rem_single, reset_seq_pointers, etc
- logs.cgi                # main page for application and driver logs and reading regs
- log0.cgi log1.cgi
- driver.cgi
- hw.cgi

cgi-bin directory have to be writeable for all ! chmod o+w cgi-bin
