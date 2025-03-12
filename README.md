# cpp-interpreter

Program - vyhodnocuje inštrukcie a dá sa spustiť.
Všetky možné hodnoty sú obalené vo Variant triede.
Program má v sebe zoznam inštrukcii a zoznam konštant.
Variant môže byť nič, číslo, slovník, funkcia.
Má viacero metód podľa toho ako chceme program spustiť, ale
najjednoduchšia je metóda run(), ktorá spustí program a vráti výsledok.

BytecodeBuilder - vytvára inštrukcie pre program.
Uchováva si informácie o zásobniku, konštantách a inštrukciach.
Prídáva aj zložené príkazy, ktoré sa skladajú z viacerých inštrukcii.
Keď sú všetky inštrukcie zadané tak sa môže program vytvoriť cez build().

ProgramBuilder - interpretuje kód v ľudskej podobe na inštrukcie.
Využiva pri tom AST výrazy, ktoré sú jednoduché štruktúry a vedia sa
skompilovať na symboly. Ďalej využiva symboly, ktoré reprezentujú
vyrtuálne dáta a ich vzťahy medzi nimi. Najzákladnejší symbol je
literál, ktorý má priamo svoju hodnotu avšak sú tu i všeliaké symboly,
ktoré reprezentujú výsledky nejakých operácii.
Tiež je dovolené pridavať do jazyka vlastné c++ funkcie pomocou objektu
Context a Literal::function avšak majú povolené brať len jeden argument.

    Samotný jazyk definuje kľúčové slová let, in, with, if, else
    a podporuje viaceré štruktúry ako operátory + - / * % | &
    funkcie v tvare let funkcia : arg1 arg2 ... = ... in ...
    slovníky s číslami { 1 = 4 , 5 = 6 }
    zjednotenie slovníkov s { ... } with { ... }
    získanie hodnoty zo slovníka cez # napr. { 1 = 2 } # 1
    tiež dovoľuje písať aj komenty pomocou ; napr. 11 ; toto je komentár o 11
    Ako hodnota nič sa používajú prazdne zátvorky () v zdroji pod názvom Unit.
    Vetvenie sa je možné písať ako:

let x = hodnota1 if podmienka1 else

        hodnota2 if podmienka2 else

        hodnota3

Jazyk je slepý voči prázdnym znakom avšak je potrebné, aby každý operátor bol odedelený medzerami zprava aj zľava inak sa môže stať že sa bude považovať za súčasť názovu premennej.

Premenné sa môžu skaladať zo všetkých znakov okrem medzier, ale musia začínať nejakým písmenom abecedy teda premenna145' je dovolená ale 0var nie.

