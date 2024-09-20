<script setup lang="ts">
const props = defineProps({
  class: {
    type: String,
  },
  layoutClass: {
    type: String,
  },
});
</script>

<template>
  <div class="slidev-layout">
    <div class="title mb-6" :class="props.layoutClass">
      <slot />
    </div>
    <div
      class="three-columns w-full h-full grid grid-rows-3 gap-x-12 gap-y-20 items-stretch"
      :class="props.layoutClass"
    >
      <div id="top" class="grid grid-cols-3 items-stretch">
        <div class="col-top-left" :class="props.class">
          <slot name="top-left" />
        </div>
        <div class="col-top-middle" :class="props.class">
          <slot name="top-middle" />
        </div>
        <div class="col-top-right" :class="props.class">
          <slot name="top-right" />
        </div>
      </div>
      <div id="bottom" class="grid grid-cols-3 items-stretch">
        <div class="col-bottom-left" :class="props.class">
          <slot name="bottom-left" />
        </div>
        <div class="col-bottom-middle" :class="props.class">
          <slot name="bottom-middle" />
        </div>
        <div class="col-bottom-right" :class="props.class">
          <slot name="bottom-right" />
        </div>
      </div>
    </div>
  </div>
</template>
