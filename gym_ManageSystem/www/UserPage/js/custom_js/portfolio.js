"use strict";
 $(document).ready(function() {
     $(".hover-btn button").click(function() {
         $(".hover-btn").remove();
     });
 });

 $(function() {
     gallery();
 });

 function gallery() {
     function mixitup() {
         $("#gallery").mixItUp({
             animation: {
                 duration: 300,
                 effects: "fade translateZ(-360px) stagger(34ms)",
                 easing: "ease",

             }
         });
     }

     mixitup();
 }

 $(document).ready(function() {
     $(".fa-eye").click(function() {
         $(this).closest('.hover-btn').hide();
     });
     $(".box-shadow").hover(function(){
         $(".hover-btn").show();
     });

     $('.fancybox-buttons').fancybox({
         openEffect: 'none',
         closeEffect: 'none',

         prevEffect: 'none',
         nextEffect: 'none',

         closeBtn: false,

         helpers: {
             title: {
                 type: 'inside'
             },
             buttons: {}
         },

         afterLoad: function() {
             this.title = 'Image ' + (this.index + 1) + ' of ' + this.group.length + (this.title ? ' - ' + this.title : '');
         }
     });

 });
