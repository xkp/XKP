/**
 * agrega validacion por numeros
 */
jQuery.fn.keyValidateByClass = function () {
    var t = $(this);
    if (t.hasClass('number')) {
        t.keypress(function (event){

            var otrosKeys = [0,8,13];

            if (t.hasClass('decimal')) {
                otrosKeys.push(46);
            }

            if (t.hasClass('negative')) {
                otrosKeys.push(45);
            }

            if (t.hasClass('filtro')) {
                otrosKeys.push(37);
            }

            if (t.hasClass('alpha')) {
                otrosKeys.push(37);
            }

            if(jQuery.inArray(event.which, otrosKeys) == -1 && (event.which < 48 || event.which > 57)) {
                event.preventDefault();
                return false;
            }
        });
    } else if (t.hasClass('text')) {
        t.keypress(function (event){

            var otrosKeys = [13];

            if(jQuery.inArray(event.which, otrosKeys) != -1) {
                event.preventDefault();
                return false;
            }
        });
    }

    return $(this);
}