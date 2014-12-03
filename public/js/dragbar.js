$(window).load(function(){
var i = 0;
var dragging = false;
   $('#dragbar').mousedown(function(e){
       e.preventDefault();
       
       dragging = true;
       var main = $('#main');
       var ghostbar = $('<div>',
                        {id:'ghostbar',
                         css: {
                                height: main.outerHeight(),
                                top: main.offset().top,
                                left: main.offset().left
                               }
                        }).appendTo('body');
       
        $(document).mousemove(function(e){
          ghostbar.css("left",e.pageX+2);
       });
    });

   $(document).mouseup(function(e){
       if (dragging) 
       {
           $('#sidebar').css("width",e.pageX+2);
           $('#main').css("left",e.pageX+2);
           $('#ghostbar').remove();
           $(document).unbind('mousemove');
           dragging = false;
       }
    });
});